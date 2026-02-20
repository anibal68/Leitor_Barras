#include <Arduino.h>

extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "usb/usb_host.h"
#include "usb/hid_host.h"
#include "usb/hid_usage_keyboard.h"
}

struct PersonRecord {
  int id;
  const char *name;
};

static const PersonRecord personDb[] = {
    {1, "Ana Silva"},
    {2, "Bruno Costa"},
    {3, "Carla Santos"},
    {4, "Daniel Souza"},
    {5, "Eduarda Lima"},
    {6, "Felipe Rocha"},
    {7, "Gabriela Almeida"},
    {8, "Henrique Nunes"},
    {9, "Isabela Castro"},
    {10, "Joao Ribeiro"},
    {11, "Julia Fernandes"},
    {12, "Kaique Moreira"},
    {13, "Larissa Mendes"},
    {14, "Marcos Pinto"},
    {15, "Natalia Araujo"},
    {16, "Otavio Martins"},
    {17, "Patricia Teixeira"},
    {18, "Rafael Gomes"},
    {19, "Sabrina Vieira"},
    {20, "Tiago Barbosa"},
    {21, "Aline Freitas"},
    {22, "Bernardo Leal"},
    {23, "Camila Farias"},
    {24, "Diego Peixoto"},
    {25, "Evelyn Cunha"},
    {26, "Fabricio Dantas"},
    {27, "Giovana Prado"},
    {28, "Heitor Moraes"},
    {29, "Ingrid Tavares"},
    {30, "Jonas Melo"},
    {31, "Karina Paiva"},
    {32, "Leonardo Siqueira"},
    {33, "Mirela Neves"},
    {34, "Nicolas Coelho"},
    {35, "Olivia Aguiar"},
    {36, "Paulo Viana"},
    {37, "Quezia Monteiro"},
    {38, "Renato Sales"},
    {39, "Sara Azevedo"},
    {40, "Tales Arruda"},
    {41, "Adriana Brito"},
    {42, "Bianca Medeiros"},
    {43, "Cesar Lobo"},
    {44, "Debora Xavier"},
    {45, "Enzo Pacheco"},
    {46, "Flavia Mota"},
    {47, "Gustavo Chaves"},
    {48, "Helena Pires"},
    {49, "Igor Falcao"},
    {50, "Janaina Torres"},
    {51, "Alexandre Dias"},
    {52, "Beatriz Rezende"},
    {53, "Caio Nascimento"},
    {54, "Denise Correia"},
    {55, "Elias Matos"},
    {56, "Fernanda Melo"},
    {57, "Gilberto Reis"},
    {58, "Heloisa Duarte"},
    {59, "Icaro Lopes"},
    {60, "Jessica Cardoso"},
    {61, "Andre Morel"},
    {62, "Barbara Assis"},
    {63, "Cristina Braga"},
    {64, "Douglas Faro"},
    {65, "Elisa Guedes"},
    {66, "Fabiana Pontes"},
    {67, "Guilherme Luz"},
    {68, "Hugo Veloso"},
    {69, "Iris Fonseca"},
    {70, "Josue Dourado"},
    {71, "Ariane Cordeiro"},
    {72, "Breno Camargo"},
    {73, "Cintia Franco"},
    {74, "Davi Bastos"},
    {75, "Ester Ventura"},
    {76, "Fernando Senna"},
    {77, "Grazi Neri"},
    {78, "Humberto Kato"},
    {79, "Ivana Penha"},
    {80, "Jean Rangel"},
    {81, "Amanda Avelar"},
    {82, "Brenda Salomao"},
    {83, "Claudio Teles"},
    {84, "Daiane Ferraz"},
    {85, "Erick Teodoro"},
    {86, "Fatima Marins"},
    {87, "Geovana Cunha"},
    {88, "Haroldo Braga"},
    {89, "Ilana Pontual"},
    {90, "Jorge Linhares"},
    {91, "Aurea Soares"},
    {92, "Brayan Castro"},
    {93, "Cleo Nogueira"},
    {94, "Dario Cabral"},
    {95, "Emanoel Fiuza"},
    {96, "Flora Sampaio"},
    {97, "Gerson Valim"},
    {98, "Hanna Queiroz"},
    {99, "Irineu Paes"},
    {100, "Juliana Porto"}};

static constexpr size_t PERSON_DB_SIZE = sizeof(personDb) / sizeof(personDb[0]);

static char barcodeBuffer[32];
static size_t barcodeLen = 0;
static uint8_t previousKeys[HID_KEYBOARD_KEY_MAX] = {0};

static const char keycodeToChar[128] = {
    [HID_KEY_1] = '1', [HID_KEY_2] = '2', [HID_KEY_3] = '3', [HID_KEY_4] = '4', [HID_KEY_5] = '5',
    [HID_KEY_6] = '6', [HID_KEY_7] = '7', [HID_KEY_8] = '8', [HID_KEY_9] = '9', [HID_KEY_0] = '0'};

const char *findNameById(int id) {
  for (size_t i = 0; i < PERSON_DB_SIZE; i++) {
    if (personDb[i].id == id) {
      return personDb[i].name;
    }
  }
  return nullptr;
}

bool keyInArray(const uint8_t *arr, uint8_t key) {
  for (int i = 0; i < HID_KEYBOARD_KEY_MAX; i++) {
    if (arr[i] == key) {
      return true;
    }
  }
  return false;
}

void processBarcode() {
  if (barcodeLen == 0) {
    return;
  }

  barcodeBuffer[barcodeLen] = '\0';
  char *endPtr = nullptr;
  long value = strtol(barcodeBuffer, &endPtr, 10);

  if (*endPtr == '\0' && value >= 1 && value <= 100) {
    const char *name = findNameById((int)value);
    if (name != nullptr) {
      Serial.print("Codigo ");
      Serial.print(value);
      Serial.print(" -> ");
      Serial.println(name);
    } else {
      Serial.println("inexistente");
    }
  } else {
    Serial.println("inexistente");
  }

  barcodeLen = 0;
}

void processKey(uint8_t keycode) {
  if (keycode == HID_KEY_ENTER || keycode == HID_KEY_KP_ENTER) {
    processBarcode();
    return;
  }

  if (keycode >= sizeof(keycodeToChar)) {
    return;
  }

  char ch = keycodeToChar[keycode];
  if (ch == 0) {
    return;
  }

  if (barcodeLen < (sizeof(barcodeBuffer) - 1)) {
    barcodeBuffer[barcodeLen++] = ch;
  }
}

void hidInputReportHandler(const uint8_t *data, int length) {
  if (length < (int)sizeof(hid_keyboard_input_report_boot_t)) {
    return;
  }

  const hid_keyboard_input_report_boot_t *kb = reinterpret_cast<const hid_keyboard_input_report_boot_t *>(data);

  for (int i = 0; i < HID_KEYBOARD_KEY_MAX; i++) {
    uint8_t key = kb->key[i];
    if (key != 0 && !keyInArray(previousKeys, key)) {
      processKey(key);
    }
  }

  memcpy(previousKeys, kb->key, HID_KEYBOARD_KEY_MAX);
}

void hidInterfaceCallback(hid_host_device_handle_t hid_device_handle, hid_host_interface_event_t event, void *arg) {
  (void)arg;

  if (event == HID_HOST_INTERFACE_EVENT_INPUT_REPORT) {
    uint8_t data[64];
    size_t dataLength = 0;
    if (hid_host_device_get_raw_input_report_data(hid_device_handle, data, sizeof(data), &dataLength) == ESP_OK) {
      hidInputReportHandler(data, (int)dataLength);
    }
  }
}

void hidDeviceCallback(hid_host_device_handle_t hid_device_handle, hid_host_driver_event_t event, void *arg) {
  (void)arg;

  if (event == HID_HOST_DRIVER_EVENT_CONNECTED) {
    Serial.println("Leitor HID conectado");
    hid_host_device_open(hid_device_handle);
    hid_host_device_set_protocol(hid_device_handle, HID_PROTOCOL_BOOT);
    hid_host_device_start(hid_device_handle);
  } else if (event == HID_HOST_DRIVER_EVENT_DISCONNECTED) {
    Serial.println("Leitor HID desconectado");
    barcodeLen = 0;
    memset(previousKeys, 0, sizeof(previousKeys));
  }
}

void usbLibTask(void *arg) {
  (void)arg;
  while (true) {
    uint32_t eventFlags;
    usb_host_lib_handle_events(portMAX_DELAY, &eventFlags);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("Projeto: leitor barras");
  Serial.println("Aguardando scanner USB HID...");

  const usb_host_config_t hostConfig = {
      .skip_phy_setup = false,
      .intr_flags = ESP_INTR_FLAG_LEVEL1,
  };
  if (usb_host_install(&hostConfig) != ESP_OK) {
    Serial.println("Erro ao iniciar USB Host");
    return;
  }

  xTaskCreate(usbLibTask, "usb_events", 4096, nullptr, 5, nullptr);

  const hid_host_driver_config_t hidConfig = {
      .create_background_task = true,
      .bg_task_priority = 5,
      .callback = hidDeviceCallback,
      .callback_arg = nullptr,
  };
  if (hid_host_install(&hidConfig) != ESP_OK) {
    Serial.println("Erro ao iniciar HID Host");
    return;
  }

  const hid_host_device_config_t deviceConfig = {
      .dev_addr = HID_HOST_ANY_DEVICE_ADDRESS,
      .iface_num = HID_HOST_ANY_INTERFACE,
      .sub_class = HID_SUBCLASS_BOOT_INTERFACE,
      .protocol = HID_PROTOCOL_KEYBOARD,
      .callback = hidInterfaceCallback,
      .callback_arg = nullptr,
  };

  if (hid_host_device_init(&deviceConfig) != ESP_OK) {
    Serial.println("Erro ao registrar interface HID");
    return;
  }

  Serial.println("Sistema pronto. Leia codigos de 1 a 100.");
}

void loop() {
  delay(100);
}
