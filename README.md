# Low Profile Memory Scanner (LPMS)

LPMS is a light-weight memory scanner for the Windows operating system. It gives users the ability to scan, disassemble and debug the memory space of another application. The motivation behind LPMS is related to reverse-engineering.

LPMS was once on the market for sale. It was sold to hundreds of customers world-wide. The product was maintained for over a year before being discontinued. I had discontinued it so that I could focus on my University studies. 

## Usage

The memory scanner is a dynamic link library (DLL). This is because DLLs in Windows are more adept to evasive reverse-engineering than a standalone application. In order to use LPMS, you must inject the DLL into the target application. Injection in this context means loading the DLL into the memory space of the target application. I have provided two DLL injecters in my GitHub repository. The first uses the Windows API to perform injection while the latter utilizes a kernel driver for a more evasive kernel-mode injection.

Upon successful injection, the GUI of the DLL will appear and you will be able to begin reverse engineering a target application. 

### Alpha Testing

https://www.youtube.com/watch?v=ZxoxMw8RSeI&t=56s

### Final Product

https://www.youtube.com/watch?v=OiIWFUwoeLI

## Contributing

Bug reports and pull requests are welcome on GitHub at [hussain-alhomedawy](https://github.com/hussain-alhomedawy/NETPoint).

## License

The repository is available as open source under the terms of the [MIT](https://opensource.org/licenses/MIT) License
