# Brainfuck++

## Compiler
First let's talk about the compiler.
The compiler generates basic x86-64 assembly code which uses bf++ backend.
Now the assembly is assembled into object code and is statically linked with the backend.
Now, the executable is ready to run!
This assembles and links using *gcc* by default.

### Installation
1. Clone this repto
2. Run `make` which will make this in this project itself inside bin/
3. Run `make install` which moves the backend static library to `/usr/local/lib/`
   the executable into `/usr/local/bin/` (to do mannually: for the compiler to work,
   it expect the backend library (inside build/) to be in /usr/local/lib/)
4. Verify the install by typing `bfppc --version`
5. Then you could use `bfppc --help` to know more about using the compiler

## Language
Just a brainfuck advanced version.
Every brainfuck program is valid. And also got other stuff including:

### Proper Comments
This is very simple; to put a comment use '#', like in Python

### Registers
Registers will actually store value and also give it's value.
The two new symbols are:
- '=n' will set a register n's value to the value of current cell
- '$n' will set the value of current cell to the value of register r

**Example**
```
++++++++++++++++++++=10 # now register 10 holds 20
>                       # going to the next cell
$10                     # now this cell is 10
```

The maximum number of registers can be setted up at compile time.

### Random
The '?' symbol will set the value of current cell to a random number
between 0 and current value of the current cell if the current value
of the current cell > 0 else a random number between 0 and 255.

**Example**
```
++++++++++?     # now the current cell have a random number
                # between 0 and 10
>               # moving to next cell
?               # since the cell's value was 0, now the 
                # cell holds a random number between 0 and 255
```

### Exec
This might be either very good or very poor.
This will execute the current cell's value.
Didn't get it?
Letme try another way.

The value of current cell, if it's ASCII character is a valid
brainfuck++ symbol with no args like '+', '-', '>', '?', ...,
That would get executed.

If it's a thing which needs args, like '=', and '$',OR if it's
'!' (tho it's single arg, i just added it to not to have crash),
Runtime Error will happen (atleast better than raw crash)

**Example**
```
++++++++++
++++++++++
++++++++++
++++++++++
+++
# now the current cell holds 43
! # since (char)43 = '+', '+' got executed and now the value 
  # of current cell is 44!
```

### Misc
- When pointer exceeds the tape size, it silently wraps around it.
