

def compiler(filename: str) -> str:
  file = open(filename, "r")
  output = open(filename + ".cpp", "w")

  variables_remap = {}

  # Setup initial state
  output.write("#include <iostream>\n")
  output.write("#include <string>\n")
  output.write("using namespace std;\n")
  output.write("int main() {\n")

  tabs = 0
  for line in file.readlines():
    # Check if the number of tabs decreased
    index = 0
    while (line[index] == ' ' or line[index] == '\t'):
      if (line[index] == '\t'):
        index += 4
      else:
        index += 1
    if (index != tabs and not line.strip().startswith("else") and
        not line.strip().startswith("elif")):
      output.write("}")
    tabs = index

    # Remove the tabs from the line
    line = line.strip()

    # Update the old variable names with the new assigned ones
    for (key, val) in variables_remap.items():
      line = var_replace(line, key, val)

    if (line.startswith("print")):
      output.write("cout << " + line[5:] + " << endl;\n")
    elif (line.startswith("if")):
      output.write("if (" + line[2:len(line)-1] + ") {\n")
      tabs += 4
    elif (line.startswith("else")):
      output.write("} else {\n")
      tabs += 4
    elif (line.startswith("elif")):
      output.write("} else if (" + line[4:len(line)-5] + ") {\n")
      tabs += 4
    elif ("=" in line):
      ss = line.split("=")
      var = ss[0].strip()
      data = ss[1].strip()

      # Get the new variable name for reassignment
      newName = var

      while (newName in variables_remap):
        newName += "_0"

      variables_remap[var] = newName
      output.write("auto " + newName + " = " + data + ";\n")

  # Close all the open brackets
  while (tabs > 0):
    output.write("}\n")
    tabs -= 4

  output.write("return 0;}\n")

  file.close()
  output.close()
  return filename + ".cpp"

def var_replace(s: str, old: str, newWord: str) -> str:
  result = ""
  lastPos = 0
  pos = s.find(old)

  current_quote = 0 # 0 means not in quotes, otherwise ' or "
  escaped = False

  while (pos != -1):
    # Update quote/escape state for the text we are about to skip over
    for i in range(lastPos, pos):
      if (escaped):
        escaped = False
      elif (s[i] == '\\'):
        escaped = True
      elif (current_quote == 0):
        if (s[i] == '"' or s[i] == '\''):
          current_quote = s[i]
      elif (s[i] == current_quote):
        current_quote = 0

    # Append the text before the current match
    result += s[lastPos:pos]

    # Check if it's a whole word (not surrounded by legal variable chars)
    beforeIsVarChar = (pos > 0 and is_var_char(s[pos - 1]))
    afterIsVarChar = (pos + len(old) < len(s) and is_var_char(s[pos + len(old)]))

    # Only replace if we are NOT inside a string literal AND it's a whole word
    if (current_quote == 0 and not beforeIsVarChar and not afterIsVarChar):
      result += newWord
    else:
      result += old

    lastPos = pos + len(old)
    pos = s.find(old, lastPos)

  result += s[lastPos:]
  return result

def legal_var(s: str) -> bool:
  if not s:
    return False
  if not (s[0].isalpha() or s[0] == '_'):
    return False
  
  for c in s:
    if not (c.isalnum() or c == '_'):
      return False
  
  return True

def is_var_char(c: str) -> bool:
  return c == "_" or c.isalnum()