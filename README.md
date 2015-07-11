Real-time Memory Scanner (sold and published as Low Profile Memory Scanner or LPMS)

  I developped the Memory Engine with C++ while I was still a junior in high school. You may find instances in the source-code 
that could be improved on. I didn't have the time to refurbish the source-code after its release since I didn't get a chance.

  Watch out for the license-check at the main entry-point of every sub-project within the project. The Memory Engine itself is 
a DLL so in order to launch it you need to inject it into your target application. I have provided a kernel-mode DLL injector in another respository named InjecTOR and InjecTOR-Driver. The reason why I invested most of my time with a kernel-mode injector rather than a simpler user-mode injector is primarily due to the fact that using a driver is mostly undetectable by anti-hacking systems.
