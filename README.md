# at42minishell

This is a minishell, a 42 school project that reproduce a basic shell, or CLI.

We use an AST ([Abstract Syntax Tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree))!

We did all the bonuses except the `HERE_DOCUMENT` (`<<` operator).

Made by [charmstr](https://github.com/charMstr) and [me](https://github.com/mli42).

## Usage
```shell
# Compile minishell with the Makefile
make -j4

# Start the shell
./minishell
```

## Features

- [x] Line editing (<kbd>←</kbd> / <kbd>→</kbd>)
- [x] History (<kbd>↑</kbd> / <kbd>↓</kbd>)
- [x] Copy (start: <kbd>CTRL</kbd>+<kbd>K</kbd> | stop : <kbd>CTRL</kbd>+<kbd>K</kbd>)
- [x] Past (<kbd>CTRL</kbd>+<kbd>P</kbd>)
- [x] Cut (start: <kbd>CTRL</kbd>+<kbd>K</kbd> | stop : <kbd>CTRL</kbd>+<kbd>L</kbd>)

- [x] Built-ins
    - `echo`
       - `-n` flag
    - `cd` (absolute/relative path + ~/-)
    - `pwd`
    - `export`
    - `unset`
    - `env`
    - `exit`
- [x] Launch binaries from `$PATH` (of course)

- [x] Semicolons (`;`)
- [x] Pipes (`|`)
- [x] Quotes (`"`, `'`)
- [x] Redirections (`>`, `>>`, `<`)
- [x] Signals (`CTRL-C` / `CTRL-\` / `CTRL-D`)
- [x] `AND` operator (`&&`), `OR` operator (`||`)
- [x] Parenthesis priority, creates a subshell (`(cmd)`)
- [x] Environment variables (ex: `$PATH`) + `$?`

- [x] Go start of line (<kbd>CTRL</kbd>+<kbd>A</kbd> or <kbd>HOME</kbd>)
- [x] Go end of line (<kbd>CTRL</kbd>+<kbd>E</kbd> or <kbd>END</kbd>)

- [x] Navigate between words (<kbd>CTRL</kbd>+<kbd>←</kbd> and <kbd>CTRL</kbd>+<kbd>→</kbd>)
- [x] Navigate between lines (<kbd>CTRL</kbd>+<kbd>↑</kbd> and <kbd>CTRL</kbd>+<kbd>↓</kbd>)

- [x] Multiline (giving a line with unclosed quote (`"`, `'`) or backslash (`\`) prints a PS2 until the line is finished)
- [x] Pathname expansion. Wildcard `*` (unquoted)

- [x] Update `SHLVL`, the prompts `PS1` and `PS2`
- [x] Ambiguous Redirections

## Main behavior of the minishell:

the parsing happens in two disctinct STEPS
	1) lexer (lexical analizer)	=> creates tokens
	2) parser => process the tokens acording to a grammar and builds an
	abstract synthax tree of all the commands we need to run.	
	note:	we do hande the "&&" and "||" operators, as well as the parenthesis
			to start subshells.

I]	getting the next command line:
	the terminal is put in raw mode in order to have an exact bash like
	input interface.
	- we have a history of the last commands launched. (ctrl up
	and ctrl down). that could be the only difference between the real bash, if
	we go up in the history and sart to edit a command line, then change to an
	other line in history back and forth, the edition we just did gets lost.
	Except for that every features of bash is reproduced.
	note: hitting the tab key doesnt do the autocompletion of file names or
	commands.
	The trailing <backslash> and <newline> are removed before splitting the
	input into tokens.

II] tokenisation (aka lexing)

	1) char by char, create tokens with the lexer. If the tokens arent finished
	at the end of the user's input (unmatching quotes or parenthesis,
	escaped new-line...):
	--> prompt a PS2. redo it UNTIL the all the tokens are closed.
	note: fix set of metacharacter to separate tokens (unquoted and unescaped)
		<space>, <tab>, <newline>, ;, (, ), |, &, <, >.

	2) then when the token constructs are fully complete we can stop getting
	the user 's input, and start to parse the command.
	
	3) categorizing the tokens as we creat them from left to right.
	Once a token is delimited, make sure its being categorized as required by
	the shell grammar.

	4) separate in between: OPERATOR (including IO_NUMBER...) or TOKEN
	
III]	build an abstract tree in the parser section.
		thanks to the operators, we can identify the begining and end of jobs.

IV]	in each simple commands the tokens list is subject to word expansion:
	1)	tild expansion is done at the begining of a potential path, or after
		the first '=' sign that is in between a label=value pattern.
		ex: echo salut=~:~/Projects:~/hey -> 
		salut=/HOME/charmstr:/HOME/charmstr/Projects:/HOME/charmstr/hey
	2)	parameter expansion
			example: $PWD -> /home/...
	3)	field splitting, applied to the parameter expansion, only if in an
		unquoted section of text.
			example:	export hey="one       two"
						echo $hey --> one two
						the spaces are removed, expansion got split in two.
						echo "$hey" --> one      two
						the spaces arent removed. still one uniq string.
	note:	portion of the new strings that come from an expansion are
			marked as protected against late quote removal. every char is
			to be taken litteraly.
	4)	pathname expansion
			example: ls -> salut salop salt samy
					 echo sal* -> salut salop salt
	note:	it only applies if the '*' is unquoted (unescaped etc).
			if it doesnt apply, we jump to step 5.
			also note that some quote removal is done before
			this action occurs.
	5)	quote removal (not on the protected strings parts resulting
			from the parameter expansion).

V]	Redirection is performed if necessary. they are separated from the argv,
	but kept in the order of appearance. redirecting stdin and stdout temporary

VI]	Commands are executed. then a new prompt is displayed.
