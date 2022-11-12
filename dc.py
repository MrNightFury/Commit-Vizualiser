import zlib


def dc (path : str) -> str:
    compressed_content = open(path, 'rb').read()
    decompressed_content = zlib.decompress(compressed_content)
    for i in range(len(decompressed_content)):
        if decompressed_content[i] == 0:
            decompressed_content = decompressed_content[i+1:]
            break
    result = ''
    mode = 'text'
    for i in decompressed_content:
        if i == 0 and mode == 'text':
            mode = 'hex'
            c = 20
            result+=' '
            continue

        if mode == 'text':
            result += i.to_bytes(1,'little').decode()
        else:
            result += i.to_bytes(1,'little').hex()
            c-=1
            if c == 0:
                mode = 'text'
                result+='\n'
    return result
