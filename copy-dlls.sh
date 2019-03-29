#!/usr/bin/env bash

ldd farmafit.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" releases/dlls
