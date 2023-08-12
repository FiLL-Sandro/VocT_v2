#!/bin/bash

key_file="/tmp/VocT.queue"
Dictionary_pid_file="/tmp/Dictionary.pid"
Menu_pid_file="/tmp/Menu.pid"

kill_em_all()
{
	local pid;

	if [ -f "$Dictionary_pid_file" ] ; then
		pid="$(cat $Dictionary_pid_file)"
		kill -15 $pid
	fi

	# if [ -f "$Menu_pid_file" ] ; then
	# 	pid="$(cat $Menu_pid_file)"
	# 	kill -15 $pid
	# fi
}

main()
{
	touch $key_file

	./out/Dictionary/Dictionary &
	./out/Menu/Menu

	kill_em_all
}

main $@
