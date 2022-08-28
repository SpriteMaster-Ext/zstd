#!/usr/bin/env zsh

err_echo() { printf "%s\n" "$*" >&2; }

get_compiler() {
	local version="$($* --version 2> /dev/null)"

	if [ $? -ne 0 ]; then
		err_echo "'$*' was unable to be executed successfully"
		exit -1
	fi
	local version="${version%%$'\n'*}"

	set -o extendedglob
	case "$version" in
		(#i)*Clang*|*llvm*) echo clang;;
		(#i)*gcc*|*g++*) echo gcc;;
		(#i)*icc*|*intel*) echo icc;;
		*) echo unknown;;
	esac
}

compiler=$(get_compiler "$*")
echo $compiler
