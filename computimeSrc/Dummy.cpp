#include "CHIPDeviceManager.h"
#include "DeviceCallbacks.h"

#include "shell_extension/launch.h"

#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/clusters/ota-requestor/BDXDownloader.h>
#include <app/clusters/ota-requestor/OTARequestor.h>
#include <app/server/OnboardingCodesUtil.h>
#include <app/server/Server.h>

#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/examples/DeviceAttestationCredsExample.h>

#include <platform/ESP32/NetworkCommissioningDriver.h>
#include <platform/ESP32/OTAImageProcessorImpl.h>
#include <platform/GenericOTARequestorDriver.h>

#include <lib/shell/Commands.h>
#include <lib/shell/Engine.h>
#include <lib/shell/commands/Help.h>
#include <lib/support/logging/CHIPLogging.h>
#include <lib/support/CodeUtils.h>

#include "esp_log.h"

#include "Dummy.h"
// #include "DummyBLE.h"
#include "DummyBLECLient.h"

namespace chip {
namespace Shell {
namespace {

Shell::Engine sSubShell;

#define TAG "DUMMY"

CHIP_ERROR shellCmdHandlerBLE(int argc, char ** argv)
{
	CHIP_ERROR error  = CHIP_NO_ERROR;
    VerifyOrReturnError(argc == 1, error = CHIP_ERROR_INVALID_ARGUMENT);

    // ESP_LOGE(TAG, "BLE test stage %s\n", argv[0]);
    // dummyBLEMain(argv[0]);
    dummyBleClientCmd(argv[0]);

    return error;
}

CHIP_ERROR shellCmdHandlerEVC(int argc, char ** argv)
{
    CHIP_ERROR error  = CHIP_NO_ERROR;
    VerifyOrReturnError(argc == 1, error = CHIP_ERROR_INVALID_ARGUMENT);

    if (strcmp(argv[0], "1") == 0)
    {
        testEVChargerCluster();
    }

    return error;
}

CHIP_ERROR DummyCmdHandler(int argc, char ** argv)
{
    if (argc == 0)
    {
        sSubShell.ForEachCommand(PrintCommandHelp, nullptr);
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR error = sSubShell.ExecCommand(argc, argv);

    if (error != CHIP_NO_ERROR)
    {
        streamer_printf(streamer_get(), "Error: %" CHIP_ERROR_FORMAT "\r\n", error.Format());
    }

    return error;
}

} // namespace

void DummyCommands::Register()
{
	    static const shell_command_t subCommands[] = {
        { &shellCmdHandlerBLE, "ble", "set BLE via ESP32 API >> Usage: dum ble <int>"},
        { &shellCmdHandlerEVC, "evc", "ev charger cluster test"},
    };

    sSubShell.RegisterCommands(subCommands, ArraySize(subCommands));

    // Register the root command in the top-level shell.
    static const shell_command_t dummyCmd[] = { 
    	{&DummyCmdHandler, "dum", "commands in dummy file"},
    };

    Engine::Root().RegisterCommands(dummyCmd, ArraySize(dummyCmd));
}

} // shell
} // ship

using namespace ::chip;
using namespace ::chip::Shell;
using namespace ::chip::Credentials;
using namespace ::chip::DeviceManager;
using namespace ::chip::DeviceLayer;

void dummyBleClient_simpleCb(void)
{
    ESP_LOGE(TAG, "call ble reset in matter");
    ConnectivityMgr().ResetInit();
}

void dummyBleClient_Delegate(Complete_u32_u32 complete)
{
    uint8_t st = (uint8_t)chip::Server::GetInstance().GetCommissioningWindowManager().CommissioningWindowStatus();

    ESP_LOGE(TAG, "Dummy Deleage Cb Ret(%d) st 0x%x", complete(0x12), st);
}

void dummyMain(void)
{
	DummyCommands & dyCmd = DummyCommands::Singleton();
    dyCmd.Register();

    dummyBleClientCmd_DelegateSet(dummyBleClient_simpleCb, dummyBleClient_Delegate);

    ESP_LOGE(TAG, "Add dummy commands!");
}

void testCluster1(void)
{
#if 0
    while(true)
    {
        chip::DeviceLayer::PlatformMgr().LockChipStack();
        int16_t temp = 0;
        chip::app::Clusters::Test2Cluster::Attributes::TestAtt1::Get(2, &temp);
        ESP_LOGE("T-Clus", "Get Att1 : %d\n\n", temp);
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        chip::DeviceLayer::PlatformMgr().LockChipStack();
        temp = 2022;
        chip::app::Clusters::Test2Cluster::Attributes::TestAtt2::Set(2, static_cast<int16_t>(temp));
        ESP_LOGE("T-Clus", "Set Att2 : %d\n\n", temp);
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        chip::DeviceLayer::PlatformMgr().LockChipStack();
        char data_str[32] = "";
        chip::MutableCharSpan data = chip::MutableCharSpan(data_str, 32);
        chip::app::Clusters::Test2Cluster::Attributes::TestAtt3::Get(2, data);
        ESP_LOGE("T-Clus", "Get Att2 : %s\n\n", data.data());
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
#endif
}

#define TEST_DELAY (250)
void testEVChargerCluster(void)
{
#if 1
    // chip::DeviceLayer::PlatformMgr().LockChipStack();
    uint32_t data = 0;
    chip::app::Clusters::EvCharger::Attributes::CurrentLimit::Get(2, &data);
    ESP_LOGE("EV-C", "Get CurrentLimit(R) : %d\n\n", data);
    // chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    vTaskDelay(TEST_DELAY / portTICK_PERIOD_MS);

    // chip::DeviceLayer::PlatformMgr().LockChipStack();
    data = 4321;
    chip::app::Clusters::EvCharger::Attributes::CurrentLimit::Set(2, static_cast<uint32_t>(data));
    ESP_LOGE("EV-C", "Set CurrentLimit(R) : %d\n\n", data);
    // chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    vTaskDelay(TEST_DELAY / portTICK_PERIOD_MS);

    // chip::DeviceLayer::PlatformMgr().LockChipStack();
    data = 0;
    chip::app::Clusters::EvCharger::Attributes::ChargeCurrent::Get(2, &data);
    ESP_LOGE("EV-C", "Get ChargeCurrent(R/W) : %d\n\n", data);
    // chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    vTaskDelay(TEST_DELAY / portTICK_PERIOD_MS);


    // chip::DeviceLayer::PlatformMgr().LockChipStack();
    data = 1234;
    chip::app::Clusters::EvCharger::Attributes::ChargeCurrent::Set(2, static_cast<uint32_t>(data));
    ESP_LOGE("EV-C", "Set ChargeCurrent(R/W) : %d\n\n", data);
    // chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    vTaskDelay(TEST_DELAY / portTICK_PERIOD_MS);

    // chip::DeviceLayer::PlatformMgr().LockChipStack();
    data = 0;
    chip::app::Clusters::EvCharger::Attributes::ChargePower::Get(2, &data);
    ESP_LOGE("EV-C", "Get ChargePower(R/W) : %d\n\n", data);
    // chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    vTaskDelay(TEST_DELAY / portTICK_PERIOD_MS);
#endif
}