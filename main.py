import sys
import subprocess
import Comp.compiler

EXECUTECOMMAND = "/opt/homebrew/bin/g++-15 -std=c++20 -fmodules-ts -Wall -g"

def main():
    for i in range(1, len(sys.argv)):
        # Run compiler to generate .cpp file
        compiled_file = Comp.compiler.compiler(sys.argv[i])
        
        # Build the command as a list to avoid shell issues and space-in-path errors
        cmd = EXECUTECOMMAND.split() + [compiled_file, "-o", "test"]
        
        # Run the compilation command
        subprocess.run(cmd, check=True)
        
        # Execute the generated 'test' binary
        subprocess.run(["./test"], check=True)

if __name__ == "__main__":
    main()