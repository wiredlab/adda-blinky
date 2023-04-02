# ADDA Blinky


Analog

```mermaid
flowchart LR

I[Analog\ninput] --> ADC[ADC]
ADC -->|8b| D[LED[7..0]]
D -->|8b| DAC[DAC]
DAC --> O[Analog\noutput]
```
