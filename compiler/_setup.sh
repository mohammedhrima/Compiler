#!/bin/bash

BASHRC_PATH="$HOME/.bashrc"
# Check if the aliases are already present
if ! grep -q "run" "$BASHRC_PATH" ; then
    cat <<EOL >> "$BASHRC_PATH"

export PS1="\\W% "
_run() {
    DEBUG_MODE=true
    cc main.c -lm -fsanitize=address -fsanitize=null -g3 -D DEBUG=\$DEBUG_MODE -o comp
    if [ \$? -eq 0 ]; then
        ./comp \$1    
        if [ \$? -eq 0 ]; then
            cc file.s -o asm        
            if [ \$? -eq 0 ]; then
                ./asm
                rm -rf asm
            fi
        fi
    fi
    rm -rf comp
}

_copy() {
    DEBUG_MODE=false
    cc main.c -lm -fsanitize=address -fsanitize=null -g3 -D DEBUG=\$DEBUG_MODE -o comp
    ./comp file.hr

    file_count=\$(ls 1.tests/*.hr 2>/dev/null | wc -l)
    next_file_number=\$((file_count + 1))
    new_filename="\$(printf '%03d' "\$next_file_number")"

    new_hr_file="1.tests/\${new_filename}.hr"
    new_s_file="1.tests/cmp/\${new_filename}.s"
    echo "create \$new_hr_file"
    echo "create \$new_s_file"
    cp "file.hr" "\$new_hr_file"
    cp "file.s" "\$new_s_file"
    rm -rf comp
}

_test() {
    DEBUG_MODE=false
    cc main.c -lm -fsanitize=address -fsanitize=null -g3 -D DEBUG=\$DEBUG_MODE -o comp
    cp comp 1.tests/

    GREEN='\033[0;32m'
    RED='\033[0;31m'
    NC='\033[0m'

    cd 1.tests || exit
    for file in *.hr; do
        if [ -f "\$file" ]; then
            # Execute the command to generate file.s
            ./comp "\$file"
            # Extract the file name without extension
            filename=\$(basename -- "\$file")
            filename_no_extension="\${filename%.*}"
            if cmp -s "file.s" "cmp/\${filename_no_extension}.s"; then
                echo -e "\${GREEN}SUCCESS\${NC} for \$file"
            else
                echo -e "\${RED}FAILED\${NC} for \$file"
            fi
        fi
    done
    rm -rf comp file.s
    cd ..
    rm -rf comp
}

_progress() {
    file_count=\$(ls 2.progress/*.c 2>/dev/null | wc -l)
    next_file_number=\$((file_count + 1))
    new_filename="\$(printf '%03d' "\$next_file_number")"
    new_c_file="2.progress/\${new_filename}.c"
    cp "main.c" "\$new_c_file"
}

_asm() {
    cc \$1 -S -fno-verbose-asm -fno-asynchronous-unwind-tables -masm=intel
}

alias run="_run \$1"
alias copy="_copy"
alias test="_test"
alias prog="_progress"
alias asm="_asm"
EOL
fi

tail -f > /dev/null