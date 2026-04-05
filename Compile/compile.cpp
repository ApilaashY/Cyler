export module compile;

import <string>;
import <fstream>;
import <unordered_map>;
import <sstream>;
import <iostream>;

std::string strip(std::string s);
std::string var_replace(std::string s, std::string old, std::string newWord);
bool legal_var(std::string s);
bool is_var_char(char c);

export std::string compile(std::string filename) {
  std::ifstream file(filename);
  std::ofstream output(filename + ".cpp");

  std::unordered_map<std::string, std::string> variables_remap;

  // Setup initial state
  output << "#include <iostream>\n";
  output << "#include <string>\n";
  output << "using namespace std;\n";
  output << "int main() {\n";

  std::string line;
  int tabs = 0;
  while (getline(file, line)) {
    // Check if the number of tabs decreased
    int index = 0;
    while (line[index] == ' ' || line[index] == '\t') {
      index += (line[index] == '\t' ? 4 : 1);
    }
    if (index != tabs && !strip(line).starts_with("else") &&
        !strip(line).starts_with("elif")) {
      output << "}" << std::endl;
    }
    tabs = index;

    // Remove the tabs from the line
    line = strip(line);

    // Update the old variable names with the new assigned ones
    for (auto const &[key, val] : variables_remap) {
      line = var_replace(line, key, val);
    }

    if (line.starts_with("print")) {
      output << "cout << " << line.substr(5) << " << endl;\n";
    } else if (line.starts_with("if")) {
      output << "if (" << line.substr(2, line.size() - 3) << ") {\n";
      tabs += 4;
    } else if (line.starts_with("else")) {
      output << "} else {\n";
      tabs += 4;
    } else if (line.starts_with("elif")) {
      output << "} else if (" << line.substr(4, line.size() - 5) << ") {\n";
      tabs += 4;
    } else if (line.find("=") != std::string::npos) {
      std::stringstream ss(line);
      std::string var, data;

      std::getline(ss, var, '=');
      std::getline(ss, data, '=');
      var = strip(var);
      data = strip(data);

      // Get the new variable name for reassignment
      std::string newName = var;

      while (variables_remap.contains(newName)) {
        newName += "_0";
      }

      variables_remap[var] = newName;
      output << "auto " << newName << " = " << data << ";\n";
    }
  }

  // Close all the open brackets
  while (tabs > 0) {
    output << "}\n";
    tabs -= 4;
  }

  output << "return 0;}\n";

  return filename + ".cpp";
}

std::string strip(std::string s) {
  const std::string whitespaces = " \t\n\r";
  size_t first = s.find_first_not_of(whitespaces);
  if (std::string::npos == first) {
    return "";
  }
  size_t last = s.find_last_not_of(whitespaces);
  return s.substr(first, (last - first + 1));
}

std::string var_replace(std::string s, std::string old, std::string newWord) {
  std::string result;
  size_t lastPos = 0;
  size_t pos = s.find(old);

  char current_quote = 0; // 0 means not in quotes, otherwise ' or "
  bool escaped = false;

  while (pos != std::string::npos) {
    // Update quote/escape state for the text we are about to skip over
    for (size_t i = lastPos; i < pos; ++i) {
      if (escaped) {
        escaped = false;
      } else if (s[i] == '\\') {
        escaped = true;
      } else if (current_quote == 0) {
        if (s[i] == '"' || s[i] == '\'') {
          current_quote = s[i];
        }
      } else if (s[i] == current_quote) {
        current_quote = 0;
      }
    }

    // Append the text before the current match
    result.append(s, lastPos, pos - lastPos);

    // Check if it's a whole word (not surrounded by legal variable chars)
    bool beforeIsVarChar = (pos > 0 && is_var_char(s[pos - 1]));
    bool afterIsVarChar =
        (pos + old.length() < s.length() && is_var_char(s[pos + old.length()]));

    // Only replace if we are NOT inside a string literal AND it's a whole word
    if (current_quote == 0 && !beforeIsVarChar && !afterIsVarChar) {
      result.append(newWord);
    } else {
      result.append(old);
    }

    lastPos = pos + old.length();
    pos = s.find(old, lastPos);
  }

  result.append(s, lastPos, std::string::npos);
  return result;
}

bool legal_var(std::string s) {
  if (s.empty())
    return false;
  if (!isalpha(s[0]) && s[0] != '_')
    return false;
  for (char c : s) {
    if (!isalnum(c) && c != '_')
      return false;
  }
  return true;
}

bool is_var_char(char c) { return isalnum(c) || c == '_'; }