#!/bin/bash

Xephyr -br -ac -noreset -screen 800x600 :2 &
sleep 3s


DISPLAY=:2 xcompmgr &
sleep 2s

DISPLAY=:2 ./basic_wm &
sleep 3s

DISPLAY=:2 xeyes &
sleep 3s

DISPLAY=:2 xterm &
sleep 3s

DISPLAY=:2 xcalc &
sleep 3s

DISPLAY=:2 xclock &



