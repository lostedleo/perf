#!/bin/bash
. env.sh
if [ $# == 0 ];then
	cd src;
	make;
	cd ..;
    if [ ! -d ./obj ];then
        mkdir obj
    fi
	make -f makeobj;
    if [ ! -d ./inc ];then
        mkdir inc
    fi
    cp src/*.h ./inc
    
elif [ $# == 1 ];then
	if [ $1 == 'clean' ];then
		rm -f obj/*
		cd src;
		make clean;
        cd ../inc;
        rm -rf *.h
	else
		echo "$0 or $0 clean"
	fi
fi
