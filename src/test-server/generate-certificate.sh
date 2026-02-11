#!/bin/sh

openssl req -x509 -newkey rsa:4096 -sha256 -days 365 -nodes \
  -keyout key.pem -out cert.pem \
  -subj "/C=US/ST=Massachusetts/L=Framingham/O=Bose Corporation/CN=audionotificationdev.api.bosecm.com" \
  -addext "subjectAltName=DNS:audionotificationdev.api.bosecm.com"
