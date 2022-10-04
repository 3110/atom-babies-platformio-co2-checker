#include "EspNowManager.h"

const char *EspNowManager::NAME = "EspNowManager";
const uint8_t EspNowManager::BROADCAST_ADDRESS[ESP_NOW_ETH_ALEN] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

EspNowManager::EspNowManager(const uint8_t channel) : CHANNEL(channel) {
    uint8_t addr[ESP_NOW_ETH_ALEN] = {0};
    esp_read_mac(addr, ESP_MAC_WIFI_STA);
    memcpy(this->address, addr, ESP_NOW_ETH_ALEN);
}

EspNowManager::~EspNowManager(void) {
}

const uint8_t *const EspNowManager::getAddress(void) const {
    return this->address;
}

bool EspNowManager::begin(void) {
    if (!WiFi.mode(WIFI_STA)) {
        return false;
    }
    if (!WiFi.disconnect()) {
        return false;
    }
    initEspNow();
    return true;
}

bool EspNowManager::registerPeer(const uint8_t *addr) {
    esp_now_peer_info_t peerInfo;
    initPeer(addr, peerInfo);
    esp_err_t e = esp_now_add_peer(&peerInfo);
    printEspErr(e);
    return e == ESP_OK;
}

bool EspNowManager::unregisterPeer(const uint8_t *addr) {
    esp_err_t e = esp_now_del_peer(addr);
    printEspErr(e);
    return e == ESP_OK;
}

bool EspNowManager::registerCallback(esp_now_recv_cb_t cb) {
    esp_err_t e = esp_now_register_recv_cb(cb);
    printEspErr(e);
    return e == ESP_OK;
}

bool EspNowManager::registerCallback(esp_now_send_cb_t cb) {
    esp_err_t e = esp_now_register_send_cb(cb);
    printEspErr(e);
    return e == ESP_OK;
}

bool EspNowManager::send(const uint8_t *peerAddr, const uint8_t *data,
                         size_t len) {
    esp_err_t e = esp_now_send(peerAddr, data, len);
    printEspErr(e);
    return e == ESP_OK;
}

void EspNowManager::initEspNow(void) {
    esp_err_t e = esp_now_init();
    if (e != ESP_OK) {
        printEspErr(e);
        ESP.restart();
    }
}

void EspNowManager::initPeer(const uint8_t *addr, esp_now_peer_info_t &peer) {
    memset(&peer, 0, sizeof(peer));
    memcpy(&(peer.peer_addr), addr, ESP_NOW_ETH_ALEN);
    peer.channel = this->CHANNEL;
    peer.encrypt = 0;
}

void EspNowManager::printEspErr(esp_err_t e) {
    switch (e) {
        case ESP_OK:
            SERIAL_PRINTLN("OK");
            break;
        case ESP_ERR_ESPNOW_NOT_INIT:
            SERIAL_PRINTLN("ESPNOW is not initialized");
            break;
        case ESP_ERR_ESPNOW_ARG:
            SERIAL_PRINTLN("invalid argument");
            break;
        case ESP_ERR_ESPNOW_NO_MEM:
            SERIAL_PRINTLN("Out of memory");
            break;
        case ESP_ERR_ESPNOW_FULL:
            SERIAL_PRINTLN("ESPNOW peer list is full");
            break;
        case ESP_ERR_ESPNOW_NOT_FOUND:
            SERIAL_PRINTLN("ESPNOW peer is not found");
            break;
        case ESP_ERR_ESPNOW_INTERNAL:
            SERIAL_PRINTLN("Internal error");
            break;
        case ESP_ERR_ESPNOW_EXIST:
            SERIAL_PRINTLN("ESPNOW peer has existed");
            break;
        case ESP_ERR_ESPNOW_IF:
            SERIAL_PRINTLN("Interface error");
            break;
        default:
            SERIAL_PRINTF_LN("Unknown error %d", e);
            break;
    }
}
