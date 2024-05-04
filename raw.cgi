#!/usr/bin/env python3

def main():
    input_file = 'raw.txt'  
    with open(input_file, 'r') as file:
        data = file.read()
    data_array = data.split('\n')
    ret_string = ""
    
    ret_string += "<html>\n<head>\n<title>Python HTML</title>\n</head>\n<body>\n"
    ret_string += "<h1>Raw Data</h1>\n"
    ret_string += "<ul>\n"
    for item in data_array:
        ret_string += f"<li>{item}</li>\n"
    ret_string += "</ul>\n"
    ret_string +="</body>\n</html>"

    print(ret_string)

if __name__ == "__main__":
    main()