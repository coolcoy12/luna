/* luna - a simple VM based programming language aimed at portability
 *
 * Copyright (c) 2021 Fossbay
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
*/

#include "luna.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *keywords[] =
    {
        "import ", "break ", "case ",
        "char ", "const ", "continue ",
        "default ", "double ", "else ",
        "enum ", "float ", "loop ",
        "if ", "int ", "long ",
        "return ", "short ", "struct ",
        "switch ", "alias ", "unsigned ",
        "volatile ", "while ", NULL};

char *luna_compile(char *code)
{
    int index = 0;
    char storage[1024];

    int token_index = 0;
    char **tokens = malloc(sizeof(char *));

    int i, j;
    for (i = 0; code[i] != 0; i++)
    {
        char c = code[i];

        switch (c)
        {
        case '!':
        case '#':
        case '%':
        case '^':
        case '&':
        case '*':
        case '(':
        case ')':
        case '-':
        case '+':
        case '=':
        case '{':
        case '}':
        case '[':
        case ']':
        case ':':
        case ';':
        case '\"':
        case '\'':
        case '<':
        case '>':
        case ',':
        case '.':
        case '?':
        case '|':
        case '\\':
        {
            break;
        }
        }

        storage[index] = c;
        storage[index + 1] = 0;
        index++;
    }
}

byte_t *luna_assemble(char *code, int options)
{
}

int luna_execute(byte_t *program, struct luna_rt *rt)
{
    /* create a stack & call stack */
    /* bytes will be in this format: first 4 bits are instruction, second 4 bits are argument (leading bytes can be args too) */
    /* instructions
     - push (0001) - push a value to stack
     - pop (0010) - remove something off the top of the stack
     - jmp (0011) - jump to a address specified by next byte and continue executing
     - call (0100) - jump to address specified in next byte and store the previous location of call stack
     - ret (0101) - return to most recent location stored on call stack, if there isnt any, exit program

     operation instructions
     - add (0110) - add the two top values on stack and push result onto stack
     - sub (0111) - subtract the two top values on stack and push result onto stack
     - mul (1000) - multiply the two top values on stack and push result onto stack
     - div (1001) - divide the two top values on stack and push result onto stack
     - mod (1010) - modulus the two top values on stack and push result onto stack
    */

    byte_t stack[1024]; /* represents the stack */
    int sp = 0;         /* initial index of the stack pointer */

    struct luna_header *header = (struct luna_header *)program;

    printf("Count: %d, Entry: %d\n", header->count, header->entry_point);

    int i;
    for (i = 8 + header->entry_point; i < header->count + 8; i++)
    {
        byte_t byte = program[i];

        switch (byte >> 4)
        {

        case PUSH:
        {
            byte = byte & 0x0F;

            if (byte == 1)
            {
                stack[sp] = program[i + 1]; /* Push the value to the stack */
                sp++;

                i++;

                printf("PUSH %d\n", stack[sp - 1]);
            }
            else
            {
                rt->error("unsupported size for push");
                return 1;
            }
            break;
        }

        /* Maths bytecodes */
        case ADD:
        {
            if (sp < 2)
            {
                rt->error("ADD: two numbers needed");
                return 1;
            }

            /* Add the two top values of the stack and push the result to the stack */
            int a = stack[sp - 1];
            int b = stack[sp - 2];
            sp -= 2;

            stack[sp] = a + b;
            sp++;

            printf("ADD(%d,%d) = %d\n", a, b, stack[sp - 1]);
            break;
        }

        case SUB:
        {
            if (sp < 2)
            {
                rt->error("SUB: two numbers needed");
                return 1;
            }

            /* Substract the two top values of the stack and push the result to the stack */

            int a = stack[sp - 1];
            int b = stack[sp - 2];

            sp -= 2;

            stack[sp] = a - b;
            sp++;

            printf("SUB(%d,%d) = %d\n", a, b, stack[sp - 1]);

	    break;
        }

        case MUL:
        {
            if (sp < 2)
            {
                rt->error("MUL: two numbers needed");
                return 1;
            }

            /* Multiply the two top values of the stack and push the result to the stack */

            int a = stack[sp - 1];
            int b = stack[sp - 2];

            sp -= 2;

            stack[sp] = a * b;
            sp++;

            printf("MUL(%d,%d) = %d\n", a, b, stack[sp - 1]);

	    break;
        }

        case DIV:
        {
            if (sp < 2)
            {
                rt->error("DIV: two numbers needed");
                return 1;
            }

            /* Divide the two top values of the stack and push the result to the stack */

            int a = stack[sp - 1];
            int b = stack[sp - 2];

            sp -= 2;

            stack[sp] = a / b;
            sp++;

            printf("DIV(%d,%d) = %d\n", a, b, stack[sp - 1]);

	    break;
        }
	
        case VMCALL:
        {
            if (sp < 1)
            {
                rt->error("vmcall id needed");
                return 1;
            }

            sp--;
            int vmcode = stack[sp];

            if (vmcode == 1)
            {
                if (sp < 1)
                {
                    rt->error("vmcall needs argument");
                    return 1;
                }

                sp--;
                rt->write(stack[sp], "Hello, world!\n", 14);
            }
            break;
        }

        default:
            rt->error("invalid opcode");
            break;
            return 1;
        }
    }

    return stack[sp - 1];
}
