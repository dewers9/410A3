#!/usr/bin/env python3
import sys
from bs4 import BeautifulSoup
import urllib.parse
def main():

    if len(sys.argv) != 2:
        print("Usage: python script_name.py 'html_text'")
        return
    text = urllib.parse.unquote(sys.argv[1])

    try:

        soup = BeautifulSoup(text, 'html.parser')
        print(str(soup))
        return

    except Exception as e:
        print("False")
        return
if __name__ == "__main__":
    main()