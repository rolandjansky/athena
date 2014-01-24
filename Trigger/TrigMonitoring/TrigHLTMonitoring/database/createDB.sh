#!/usr/bin/env zsh

rm test.db
sqlite3 test.db < ../database/schema.sql

