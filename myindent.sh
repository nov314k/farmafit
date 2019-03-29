#!/usr/bin/env bash

if [ -z "$1" ]; then
	echo "ERROR: Please provide a directory where files are located in."
	exit 126
else
	files=$(find $1 -name '*.c' -or -name '*.h')
	dos2unix $files
	indent $files -linux -i4 -ts4
	dos2unix $files
fi
