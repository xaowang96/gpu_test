"""
MIT License

Copyright (c) 2023 - 2024 _VIFEXTech

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import sys
import os
import re

def generate_test_case_file(test_case_name):
    # Define the template and output file names
    template_file = '_vg_lite_test_case_template.c'
    output_file = f'vg_lite_test_case_{test_case_name}.c'
    
    # Check if the template file exists
    if not os.path.exists(template_file):
        print(f"Template file '{template_file}' does not exist.")
        return

    # Check if the output file already exists
    if os.path.exists(output_file):
        response = input(f"File '{output_file}' already exists. Do you want to overwrite it? (yes/no): ")
        if response.lower() != 'yes':
            print("Operation canceled.")
            return
    
    # Read the content of the template file and perform the replacement
    with open(template_file, 'r', encoding='utf-8') as file:
        template_content = file.read()
    
    # Replace all occurrences of "template" with the user-specified name
    updated_content = template_content.replace('template', test_case_name)

    # Write the updated content to the new test file
    with open(output_file, 'w', encoding='utf-8') as file:
        file.write(updated_content)

    print(f"Test file generated: {output_file}")

def update_inc_file():
    inc_file = 'vg_lite_test_case.inc'
    
    # Generate the header comment
    header_comment = (
        "/**\n"
        " * This file is auto-generated by test_case_gen.py. Do not modify manually.\n"
        " */\n"
    )
    
    # Gather ITEM_DEF entries
    item_defs = []
    pattern = re.compile(r'vg_lite_test_case_(\w+)\.c')
    
    for filename in os.listdir('.'):
        match = pattern.match(filename)
        if match:
            item_defs.append(f"ITEM_DEF({match.group(1)})")

    # Write to the .inc file
    with open(inc_file, 'w', encoding='utf-8') as file:
        file.write(header_comment)
        for item_def in item_defs:
            file.write(f"{item_def}\n")

    print(f"Updated {inc_file} with ITEM_DEF entries.")

if __name__ == "__main__":
    # Check the number of command line arguments
    if len(sys.argv) > 2 or (len(sys.argv) == 2 and sys.argv[1] == '-h'):
        print("Usage: python test_case_gen.py [<test_name>]")
        sys.exit(1)

    # Get the test name from the command line argument if provided
    test_case_name = sys.argv[1] if len(sys.argv) == 2 else None
    
    if test_case_name:
        generate_test_case_file(test_case_name)

    update_inc_file()  # Only refresh the inc file if no test_case_name is provided
