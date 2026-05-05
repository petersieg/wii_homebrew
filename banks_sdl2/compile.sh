#!/bin/sh
clang -o banks_sdl2 banks_sdl2.c $(sdl2-config --cflags --libs) -lm


