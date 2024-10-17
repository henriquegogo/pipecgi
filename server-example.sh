#!/bin/bash

cd cgi-bin
REGISTRATION=channels

if [ ! -e "$REGISTRATION" ]; then
  mkfifo "$REGISTRATION"
fi

echo "Application started at FIFO '$REGISTRATION'"

while true; do
  CHANNEL=$(cat $REGISTRATION)
  REQUEST=$(cat $CHANNEL)

  sleep 0.5

  if [[ -n "$CHANNEL" ]]
  then
    echo -e "Content-Type: text/plain

`date`\n\n$REQUEST" > $CHANNEL
  fi
done
