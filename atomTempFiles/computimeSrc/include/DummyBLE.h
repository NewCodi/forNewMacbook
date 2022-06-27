#pragma once

#define CheckIfTrueAndExit(trueCondition, msg) \
    do                                \
    {                                 \
    	int ret = trueCondition; \
        if (ret)            \
        {                             \
            ESP_LOGE("ERROR", "ret:(%s) msg:%s", esp_err_to_name(ret), msg);              \
            goto exit;                \
        }                             \
    } while (false)

void dummyBLEMain(char* stage);
void dummyBleRegistCb(std::function<void(int)> cb);