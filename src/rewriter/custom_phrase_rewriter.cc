// Custom Phrase Rewriter implementation.
//
// This file intentionally omits Google's copyright header since it is
// new, user-authored code (not derived from existing Mozc source files).

#include "rewriter/custom_phrase_rewriter.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "converter/candidate.h"
#include "converter/segments.h"
#include "request/conversion_request.h"

namespace mozc {

CustomPhraseRewriter::CustomPhraseRewriter() { LoadTable(); }

std::string CustomPhraseRewriter::GetTsvPath() {
  const char* appdata = std::getenv("APPDATA");
  if (appdata == nullptr) {
    return "";
  }
  return std::string(appdata) + "\\Mozc\\custom_phrases.tsv";
}

void CustomPhraseRewriter::LoadTable() {
  table_.clear();

  const std::string path = GetTsvPath();
  if (path.empty()) {
    return;
  }

  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    // File not present yet; this is not an error. The user simply hasn't
    // created any custom phrases.
    return;
  }

  std::string line;
  while (std::getline(ifs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    // Trim a possible trailing '\r' (file may have CRLF line endings).
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    std::vector<absl::string_view> fields =
        absl::StrSplit(line, '\t');
    if (fields.size() < 2) {
      continue;  // Malformed line; skip.
    }

    const std::string reading(fields[0]);
    Entry entry;
    entry.value = std::string(fields[1]);
    entry.description = fields.size() >= 3 ? std::string(fields[2]) : "";

    if (reading.empty() || entry.value.empty()) {
      continue;
    }

    table_[reading].push_back(std::move(entry));
  }
}

bool CustomPhraseRewriter::Rewrite(const ConversionRequest& request,
                                   Segments* segments) const {
  if (table_.empty()) {
    return false;
  }

  bool modified = false;

  for (size_t i = 0; i < segments->conversion_segments_size(); ++i) {
    Segment* segment = segments->mutable_conversion_segment(i);
    const std::string key(segment->key());

    auto it = table_.find(key);
    if (it == table_.end()) {
      continue;
    }

    int pos = 0;
    for (const Entry& entry : it->second) {
      converter::Candidate* candidate = segment->insert_candidate(pos++);
      candidate->key = segment->key();
      candidate->content_key = candidate->key;
      candidate->value = entry.value;
      candidate->content_value = entry.value;
      candidate->description = entry.description;
    }
    modified = true;
  }

  return modified;
}

}  // namespace mozc
