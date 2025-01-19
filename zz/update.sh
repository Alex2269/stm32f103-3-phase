#!/bin/sh

   #******************************************
   git init
   git add .
   git commit -m "$(date "+%Y-%m-%d")"
   git remote add origin git@github.com:Alex2269/stm32f103-3-phase
   git push -u origin main
   git pull
   #******************************************

   #******************************************
   git status
   git add .
   git commit -m "$(date "+%Y-%m-%d")"
   git push -u origin main
   git pull
   #******************************************

   #******************************************
   git checkout -b pid-new
   git branch
   git checkout main
   #******************************************
