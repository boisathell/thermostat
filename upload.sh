#!/usr/bin/bash
IFS=$'\n'
BOARD_LIST=( $(arduino-cli board list) )
FQBN_INDEX="$(echo ${BOARD_LIST[0]} | grep -aob 'FQBN' | grep -oE '[0-9]+')"
PORT_INDEX="$(echo ${BOARD_LIST[0]} | grep -aob 'Port' | grep -oE '[0-9]+')"
POST_ARGS=${@#"$1"} 
for i in ${!BOARD_LIST[@]};
do
	filtered=$(echo ${BOARD_LIST[$i]} | grep $1)
	if [[ ! -z "${filtered// }" ]]
	then
		found_fqbn=$(echo ${filtered:$FQBN_INDEX} | awk '{print $1}')
		found_port=$(echo ${filtered:$PORT_INDEX} | awk '{print $1}')

		arduino-cli compile -b "$found_fqbn" "$POST_ARGS"
		arduino-cli upload -p "$found_port" -b "$found_fqbn" "$POST_ARGS"
	fi
done
