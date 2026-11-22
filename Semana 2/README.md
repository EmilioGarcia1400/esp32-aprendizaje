# Semana 2: LED controlado por puerto serie

Programa para ESP32 (ESP-IDF v5.5) que controla un LED con órdenes escritas en el monitor serie.

Órdenes: `prender`, `apagar` y `estado`.
Cuenta las órdenes válidas y responde "Orden no reconocida" si la palabra no coincide.
Maneja palabras demasiado largas y Enter vacío sin fallar.