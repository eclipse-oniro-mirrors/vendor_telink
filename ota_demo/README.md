## Commands

### Upload
`{u<size>` where `<size>` is firmware size in bytes. Can be generated by this command:
```bash
echo {u$(stat -c%s out/b91_devkit/ota_demo/bin/ota_demo.bin)
```

### Print
`{p<start> [<size>]` - print uploaded firmware bytes. `<size>` - first byte number. Optional `<size>` - count of printed bytes, if ommited - print one page.

### Cancel
`{cancel` - cancel update.

### Hash
`{h<start> <size>` - calculate and print sha256 hash of uploaded firmware.

### Restart
`{restart` - restart and run new firmware.

### Rollback
`{back` - restart and run previous firmware.

### Signature verification
`{s<sign>` - run signature verification, where `<sign>` is signature in hex.

#### Generate certificate:
```bash
openssl genrsa -out myprivate.pem 512
openssl rsa -in myprivate.pem -pubout > mypublic.pem
```
Write `mypublic.pem` content to file `device/soc/telink/b91/adapter/hals/update/hal_hota_board.c` as content of `mypublic_pem` variable.

#### Generate signature
```bash
openssl dgst -sha256 -sign myprivate.pem out/b91_devkit/ota_demo/bin/ota_demo.bin | xxd -ps -c200
```
Please note, you should have installed `vim` to use `xxd` command.