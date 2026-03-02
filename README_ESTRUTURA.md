# Leitor de Barras - Estrutura do Projeto

O projeto está organizado em pastas separadas por tipo de código:

## 📁 Estrutura

```
Leitor_Barras/
└── esp32/                    # Código para o microcontrolador ESP32
    ├── platformio.ini        # Configuração do PlatformIO
    ├── src/                  # Código-fonte C++
    └── main.cpp              # Arquivo principal
```

## 🎯 Como usar

### ESP32 (`esp32/`)
- Projeto PlatformIO para upload no microcontrolador
- Execute: `platformio run --project-dir esp32 -t upload`
