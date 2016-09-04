#!/bin/bash
#script args

g++ $1 `pkg-config opencv --cflags --libs` -oImagemSaida -v
./ImagemSaida $2
