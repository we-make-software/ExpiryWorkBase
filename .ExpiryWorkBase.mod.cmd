savedcmd_ExpiryWorkBase.mod := printf '%s\n'   ExpiryWorkBase.o | awk '!x[$$0]++ { print("./"$$0) }' > ExpiryWorkBase.mod
