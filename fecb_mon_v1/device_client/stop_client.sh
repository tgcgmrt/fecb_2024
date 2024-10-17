#!/bin/sh

id=`ps aux | grep deviceClientfecb |  grep -v grep | awk '{ print $2 }'`;
if [ "${id}" != "" ]
then
kill -9 $id
id=
fi

