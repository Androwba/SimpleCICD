#!/bin/bash

if [ -f status ]; then
    STATUS="$(cat status)✅"
else
    STATUS="Failed ❌"
fi

TOKEN="Private Token From BotFather"
USER_ID="833059991"

URL="https://api.telegram.org"
TEXT="Stage: $1 $STATUS%0A%0AProject:+$CI_PROJECT_NAME%0AURL:+$CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/%0ABranch:+$CI_COMMIT_REF_SLUG"

curl -s -d "chat_id=$USER_ID&disable_web_page_preview=1&text=$TEXT" $URL/bot$TOKEN/sendMessage > /dev/null
