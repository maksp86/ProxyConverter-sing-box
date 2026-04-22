#pragma once

#include <map>
#include <string>

// The headless CLI build does not compile the upstream route profile editor,
// so an empty rule-set registry is enough to satisfy shared headers.
inline const std::map<std::string, std::string> ruleSetMap {};
