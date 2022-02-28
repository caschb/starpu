#! /bin/bash

# StarPU --- Runtime system for heterogeneous multicore architectures.
#
# Copyright (C) 2021-  Université de Bordeaux, CNRS (LaBRI UMR 5800), Inria
#
# StarPU is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or (at
# your option) any later version.
#
# StarPU is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the GNU Lesser General Public License in COPYING.LGPL for more details.
#
NP=""
while true; do
	case "$1" in
		"-np")
			NP=$2
			shift 2
			;;
		"-nobind")
			export STARPU_WORKERS_NOBIND=1
			shift
			;;
		"-ncpus")
			export STARPU_NTCPIPMSTHREADS=$2
			shift 2
			;;
		"-nolocal")
			export STARPU_TCPIP_USE_LOCAL_SOCKET=0
			shift
			;;
		*)
			break
			;;
	esac
done

trap 'kill -INT $CHILDPIDS' INT
trap 'kill -QUIT $CHILDPIDS' QUIT

#echo "STARPU_TCP_MS_SLAVES=$NP $@"
STARPU_NCPUS=0 STARPU_TCP_MS_SLAVES=$NP "$@" &
CHILDPIDS="$!"

sleep 1
for i in $(seq 1 $NP):
do
	STARPU_TCP_MS_SLAVES=$NP STARPU_TCP_MS_MASTER="127.0.0.1" "$@" &
	CHILDPIDS="$CHILDPIDS $!"
done
wait %1
RET=$?
wait
exit $RET
