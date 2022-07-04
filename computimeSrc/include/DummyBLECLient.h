#pragma once

#define CheckIfTrueAndExit(trueCondition, msg) \
    do                                \
    {                                 \
    	int ret = trueCondition; \
        if (ret)            \
        {                             \
            ESP_LOGE("error", "ret:(%s) msg:%s", esp_err_to_name(ret), msg);              \
            goto exit;                \
        }                             \
    } while (false)


typedef std::function<uint32_t(uint32_t)> Complete_u32_u32;
typedef std::function<void(Complete_u32_u32)> CallbackWithComplete;
typedef std::function<void(void)> SimplyCb;

void dummyBleClientCmd(char* cmd);
void dummyBleClientCmd_DelegateSet(SimplyCb cb, CallbackWithComplete cbC);