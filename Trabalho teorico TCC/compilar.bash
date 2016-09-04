#!/bin/bash
#script args

g++ $1 `pkg-config opencv --cflags --libs` -oImagemSaida
./ImagemSaida $2
