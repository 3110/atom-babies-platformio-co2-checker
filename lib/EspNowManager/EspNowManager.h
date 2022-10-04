#pragma once

// clang-format off
#include <WiFi.h>
#include <esp_now.h>
#include "Debug.h"
// clang-format on

class EspNowManager {
public:
    static const char *NAME;
    static const uint8_t BROADCAST_ADDRESS[ESP_NOW_ETH_ALEN];

    EspNowManager(const uint8_t channel = 0);
    virtual ~EspNowManager(void);

    virtual const uint8_t *const getAddress(void) const;
    virtual bool begin(void);
    virtual bool registerPeer(const uint8_t *addr);
    virtual bool unregisterPeer(const uint8_t *addr);
    virtual bool registerCallback(esp_now_recv_cb_t cb);
    virtual bool registerCallback(esp_now_send_cb_t cb);
    virtual bool send(const uint8_t *peerAddr, const uint8_t *data, size_t len);

protected:
    virtual void initEspNow(void);
    virtual void initPeer(const uint8_t *addr, esp_now_peer_info_t &peer);
    virtual void printEspErr(esp_err_t e);

private:
    const uint8_t CHANNEL;
    uint8_t address[ESP_NOW_ETH_ALEN];
};
