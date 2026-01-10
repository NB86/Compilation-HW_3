import os

# Define the output directory
OUTPUT_DIR = "generated_tests"

if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

# --- Helper for Success Output Header ---
GLOBAL_HEADER = """---begin global scope---
print (string) -> void
printi (int) -> void
"""
GLOBAL_FOOTER = "---end global scope---"

tests = {}

# ==========================================
#              SUCCESS TESTS
# ==========================================

# 1. Basic Sanity
tests["test_success_basic"] = {
    "code": """
void main() {
    int x = 5;
    printi(x);
}
""",
    "out": GLOBAL_HEADER + """main () -> void
  ---begin scope---
  x int 0
  ---end scope---
""" + GLOBAL_FOOTER
}

# 2. Valid Casting and Byte
tests["test_success_casting"] = {
    "code": """
void main() {
    int i = 200;
    byte b = (byte)i;
    int j = b; 
}
""",
    "out": GLOBAL_HEADER + """main () -> void
  ---begin scope---
  i int 0
  b byte 1
  j int 2
  ---end scope---
""" + GLOBAL_FOOTER
}

# 3. Forward Declaration
tests["test_success_forward"] = {
    "code": """
void main() {
    foo();
}
void foo() {
    print("bar");
}
""",
    "out": GLOBAL_HEADER + """main () -> void
foo () -> void
  ---begin scope---
  ---end scope---
  ---begin scope---
  ---end scope---
""" + GLOBAL_FOOTER
}

# 4. Nested Scopes
tests["test_success_nested"] = {
    "code": """
void main() {
    int a = 1;
    if (true) {
        bool b = true;
        if (b) {
            int c = 3;
        }
    }
}
""",
    "out": GLOBAL_HEADER + """main () -> void
  ---begin scope---
  a int 0
    ---begin scope---
      ---begin scope---
      b bool 1
        ---begin scope---
          ---begin scope---
          c int 2
          ---end scope---
        ---end scope---
      ---end scope---
    ---end scope---
  ---end scope---
""" + GLOBAL_FOOTER
}

# 5. Function Parameters Offsets
tests["test_success_params"] = {
    "code": """
void foo(int a, bool b, byte c) {
    int d = a;
}
void main() {
    foo(1, true, 10b);
}
""",
    "out": GLOBAL_HEADER + """foo (int,bool,byte) -> void
main () -> void
  ---begin scope---
  a int -1
  b bool -2
  c byte -3
  d int 0
  ---end scope---
  ---begin scope---
  ---end scope---
""" + GLOBAL_FOOTER
}

# ==========================================
#              FAILURE TESTS
# ==========================================

# 6. Variable Not Defined
tests["test_fail_undef_var"] = {
    "code": """
void main() {
    x = 5;
}
""",
    "out": "line 2: variable x is not defined"
}

# 7. Function Not Defined
tests["test_fail_undef_func"] = {
    "code": """
void main() {
    foo();
}
""",
    "out": "line 2: function foo is not defined"
}

# 8. Variable Redefinition (Same Scope)
tests["test_fail_redef_var"] = {
    "code": """
void main() {
    int x;
    bool x;
}
""",
    "out": "line 3: symbol x is already defined"
}

# 9. Variable Shadowing Parameter
# Fixed: Added 'return;' to main to satisfy grammar (non-empty body)
tests["test_fail_shadow_param"] = {
    "code": """
void foo(int x) {
    int x = 5;
}
void main() {
    return;
}
""",
    "out": "line 2: symbol x is already defined"
}

# 10. Nested Shadowing
tests["test_fail_nested_shadowing"] = {
    "code": """
void main() {
    int x = 5;
    if (true) {
        int x = 10;
    }
}
""",
    "out": "line 4: symbol x is already defined"
}

# 11. Function Redefinition
# Fixed: Added 'return;' to both functions to satisfy grammar
tests["test_fail_redef_func"] = {
    "code": """
void foo() { return; }
int foo() { return 1; }
void main() { return; }
""",
    "out": "line 2: symbol foo is already defined"
}

# 12. Type Mismatch: Assign Bool to Int
tests["test_fail_assign_types"] = {
    "code": """
void main() {
    int x = true;
}
""",
    "out": "line 2: type mismatch"
}

# 13. Type Mismatch: Implicit Int to Byte
tests["test_fail_int_to_byte"] = {
    "code": """
void main() {
    int x = 5;
    byte b = x;
}
""",
    "out": "line 3: type mismatch"
}

# 14. Type Mismatch: If Condition
tests["test_fail_if_cond"] = {
    "code": """
void main() {
    if (5) {
        print("error");
    }
}
""",
    "out": "line 2: type mismatch"
}

# 15. Type Mismatch: While Condition
tests["test_fail_while_cond"] = {
    "code": """
void main() {
    int x = 0;
    while (x) {
        x = x - 1;
    }
}
""",
    "out": "line 3: type mismatch"
}

# 16. Byte Value Too Large
tests["test_fail_byte_large"] = {
    "code": """
void main() {
    byte b = 300b;
}
""",
    "out": "line 2: byte value 300 out of range"
}

# 17. Break Outside Loop
tests["test_fail_break"] = {
    "code": """
void main() {
    if (true) {
        break;
    }
}
""",
    "out": "line 3: unexpected break statement"
}

# 18. Continue Outside Loop
tests["test_fail_continue"] = {
    "code": """
void main() {
    continue;
}
""",
    "out": "line 2: unexpected continue statement"
}

# 19. Return Value in Void Function
tests["test_fail_return_val_in_void"] = {
    "code": """
void main() {
    return 5;
}
""",
    "out": "line 2: type mismatch"
}

# 20. Return Void in Non-Void Function
# Fixed: Added 'return;' to main
tests["test_fail_return_void_in_int"] = {
    "code": """
int foo() {
    return;
}
void main() {
    return;
}
""",
    "out": "line 2: type mismatch"
}

# 21. Argument Count Mismatch
# Fixed: Added 'return;' to foo
tests["test_fail_arg_count"] = {
    "code": """
void foo(int a) { return; }
void main() {
    foo(1, 2);
}
""",
    "out": "line 3: prototype mismatch, function foo expects parameters (int)"
}

# 22. Argument Type Mismatch
# Fixed: Added 'return;' to foo
tests["test_fail_arg_type"] = {
    "code": """
void foo(int a) { return; }
void main() {
    foo(true);
}
""",
    "out": "line 3: prototype mismatch, function foo expects parameters (int)"
}

# 23. Using Variable as Function
tests["test_fail_var_as_func"] = {
    "code": """
void main() {
    int x = 5;
    x();
}
""",
    "out": "line 3: symbol x is a variable"
}

# 24. Using Function as Variable
# Fixed: Added 'return;' to foo
tests["test_fail_func_as_var"] = {
    "code": """
void foo() { return; }
void main() {
    int x = foo;
}
""",
    "out": "line 3: symbol foo is a function"
}

# 25. Main Missing
# Fixed: Added 'return;' to foo
tests["test_fail_no_main"] = {
    "code": """
void foo() { return; }
""",
    "out": "Program has no 'void main()' function"
}

# 26. Main with Arguments (Invalid signature)
# Fixed: Added 'return;' so it passes syntax check
tests["test_fail_main_args"] = {
    "code": """
void main(int x) {
    return;
}
""",
    "out": "Program has no 'void main()' function"
}

# 27. Main with wrong return type
# Fixed: Syntax was actually ok here (return 0 is a statement), but kept for consistency
tests["test_fail_main_ret"] = {
    "code": """
int main() {
    return 0;
}
""",
    "out": "Program has no 'void main()' function"
}

# ==========================================
#              GENERATION LOOP
# ==========================================

count = 0
for name, data in tests.items():
    # Write .in file
    in_path = os.path.join(OUTPUT_DIR, name + ".in")
    with open(in_path, "w") as f:
        # Strip removes the initial newline, making line 1 start immediately
        f.write(data["code"].strip())
    
    # Write .out file
    out_path = os.path.join(OUTPUT_DIR, name + ".out")
    with open(out_path, "w") as f:
        f.write(data["out"].strip())
        # Add newline at end if not empty
        if data["out"].strip():
            f.write("\n")
            
    count += 1

# Using .format for Python 2.7/3.5 compatibility
print("Done! Generated {} tests in directory '{}'.".format(count, OUTPUT_DIR))
print("Make sure to run 'make' before testing.")