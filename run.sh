#!/bin/bash
rpcgen rpc_interface.x
gcc servidor.c rpc_interface_xdr.c rpc_interface_svc.c -o servidor -ltirpc -lm -fopenmp
gcc cliente.c rpc_interface_xdr.c rpc_interface_clnt.c -o cliente -ltirpc -lm
