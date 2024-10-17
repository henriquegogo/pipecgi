#!/bin/bash

REGISTRATION=channels
CHANNEL=$$
mkfifo $CHANNEL

echo "$CHANNEL" > $REGISTRATION

BODY=$(cat)

echo -e "$REQUEST_METHOD $REQUEST_URI $SERVER_PROTOCOL
Host: $HTTP_HOST
User-Agent: $HTTP_USER_AGENT
Accept: $HTTP_ACCEPT
Accept-Language: $HTTP_ACCEPT_LANGUAGE
Accept-Encoding: $HTTP_ACCEPT_ENCODING
Content-Type: $CONTENT_TYPE
Content-Length: ${#BODY}

$BODY" > $CHANNEL

timeout 5 cat $CHANNEL

rm $CHANNEL
