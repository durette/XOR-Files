# XOR-Files

Get the bitwise XOR of two files. Either write the result to a third file, or overwrite the second one.

If File 1 and File 2 are very similar...

    0101100110100101010 11 001010100101101001101
    0101100110100101010 00 001010100101101001101 1101

Then the result of XOR'ing them will be nearly all zeroes:

    0000000000000000000 11 000000000000000000000 1101

Such a file would compress easily for storage or transmission.

Later, if you have either file and the XOR, you can recover the third file.

    0101100110100101010 11 001010100101101001101
    0000000000000000000 11 000000000000000000000 1101
    0101100110100101010 11 001010100101101001101 0000

This works because...

     If A XOR B = C
     then A XOR C = B
     and B XOR C = A

For the purposes of working with different file sizes, we assume the smaller file is populated with virtual zeroes in the space above its actual allocation.

## Use Case
You have a local disk image, and you sync that image one time, in full, to cloud storage.

    baseline.img

Now, on the local disk, you keep a copy of the image that's uploaded to the cloud, and you continue working on the live image.

    baseline.img
    live.img

Later, you want to send another snapshot to the cloud. You build a snapshot.

    ./xor baseline.img live.img xor.img

If not a lot of changes have been made, this XOR file compresses well. Now you put the XOR image on the cloud, too.

    baseline.img
    xor.img.7z

Your local system crashes, and you need to roll back to the live image. The snapshot diff can be overlaid on the baseline to produce the latest version.

    ./xor baseline.img xor.img live.img


## License
Licensed under Creative Commons CC BY-SA 2.0
Written by Kevin Durette
Based on code by Philipp Hagemeister: https://stackoverflow.com/questions/6889067/how-to-perform-bitwise-operations-on-files-in-linux
