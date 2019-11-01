from distutils.core import setup, Extension

def main():
    setup(name="turbopinger",
          version="1.0.0",
          description="Blazingly-fast ipv6 pinger, made for jinglePings event and not for DDOSing.",
          author="Jakub Sadowski",
          author_email="js@vcc.earth",
          ext_modules=[Extension("turbopinger", ["turbopinger.c"])])

if __name__ == "__main__":
    main()