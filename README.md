DX UnO - Arduino Uno based Digital Modes HF QRPp Transceiver.

![DX UnO 1](https://github.com/BluQRP/DX_UnO/assets/172842404/7f909ef0-bcc3-4346-ad93-95e305b5dee4)

DX UnO is an Arduino Uno based HF Digital Modes QRPp Multiband Transceiver. DX is abbreviation for Digital Xceiver.

DX UnO is a 5 band digital modes optimized HF QRPp transceiver that can be plugged on top of an any Arduino Uno board with Atmega328p microcontroller 
to create a 400 milliwatts average RF Output power QRPp portable HF Digital Modes capable transceiver.  QRPp means the DX UnO Transceiver operates below 1 watt RF output power. 

DX UnO  can operate on any of these five bands of 20m, 17m, 15m, 12m and 10m bands and operates on popular digital modes such as  FT8, FT4, JS8call and WSPR and similar tonal modes.

DX Uno is fully CAT controlled and emulates KENWOOD TS-2000, KENWOOD TS-140S, KENWOOD TS-440S Transceivers with 9600bps to give more flexibility on CAT control rig selection.

DX UnO uses a TTL based RF PA, not PA mosfets which are prone to failure. The advantage of this TTL Logic Chip based RF Power Amplifier is that 
it is extremely resistant to SWR mishaps such as high SWR cases and antenna mishaps as antenna short or TX without antenna. 

These conditions won’t damage the RF PA section of this rig. This makes this tiny transceiver due to its resilience and small size an ideal portable rig for SOTA/POTA Activations and for beginner ham radio operators with varying antenna conditions which are prone to errors.

There is no external power supply to power DX UnO Transceiver. PC/Tablet USB connector powers the transceiver through USB cable which is also CAT control data connection.
MIC, Microphone and SPK, Speaker outputs go to relevant Microphone and Speaker outputs of PC or Tablet where the digital modes software operates.

DX UnO kit is designed to be an easy transceiver kit to solder, operate and experiment with. 
It is a suitable kit for a beginner and for seasoned ham operators to carry in their backpack for SOTA/POTA activations or for travel. The advantage of portability comes from simplicity of no need for external power supply and resilient antenna conditions RF PA.

It can be used with other microcontroller families also such as Raspberry pi or pi pico etc for experimentation. The audio output jack which is denoted as MIC has both I and Q outputs of  TAYLOE Quadrature Sampling detector which can be used to experiment with SDR SSB decoding firmware or with SDR PC applications like HDSDR.

DX UnO can be modified to work as a stand alone WSPR Transmitter by just adding a GPS module and connecting 4 wires to Arduino Uno board and loading Stand alone GPS WSPR Firmware. All details are included in DX UnO Github page. 
