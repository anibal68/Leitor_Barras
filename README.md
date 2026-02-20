# leitor barras

Projeto PlatformIO para ESP32-S3 com framework Arduino.

## Funcao
- Le leitor de codigo de barras USB em modo HID Keyboard.
- Interpreta o codigo lido como numero inteiro.
- Se for de 1 a 100, retorna o nome correspondente da base local.
- Se for diferente, retorna `inexistente`.

## Como usar
1. Abra a pasta `leitor barras` no VS Code.
2. Conecte o ESP32-S3 e o scanner USB HID (via USB OTG host).
3. Compile e grave:
   - `pio run`
   - `pio run -t upload`
4. Abra monitor serial:
   - `pio run -t monitor`

## Observacao
O scanner precisa estar em modo teclado HID (keyboard wedge).
