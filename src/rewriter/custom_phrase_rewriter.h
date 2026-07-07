// Custom Phrase Rewriter
//
// Reads a user-editable TSV file (reading TAB candidate TAB description)
// and inserts matching candidates at the top of the candidate list for
// segments whose key exactly matches a registered reading.
//
// This file intentionally omits Google's copyright header since it is
// new, user-authored code (not derived from existing Mozc source files).

#ifndef MOZC_REWRITER_CUSTOM_PHRASE_REWRITER_H_
#define MOZC_REWRITER_CUSTOM_PHRASE_REWRITER_H_

#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "converter/segments.h"
#include "request/conversion_request.h"
#include "rewriter/rewriter_interface.h"

namespace mozc {

class CustomPhraseRewriter : public RewriterInterface {
 public:
  CustomPhraseRewriter();
  ~CustomPhraseRewriter() override = default;

  bool Rewrite(const ConversionRequest& request,
               Segments* segments) const override;

 private:
  struct Entry {
    std::string value;
    std::string description;
  };

  // Loads (or reloads) the TSV definition file from the fixed path under
  // the user's AppData folder. Safe to call multiple times; re-reads the
  // file each time so edits take effect after Mozc is restarted.
  void LoadTable();

  // Returns the absolute path to custom_phrases.tsv under
  // %APPDATA%\Mozc\custom_phrases.tsv
  static std::string GetTsvPath();

  absl::flat_hash_map<std::string, std::vector<Entry>> table_;
};

}  // namespace mozc

#endif  // MOZC_REWRITER_CUSTOM_PHRASE_REWRITER_H_
