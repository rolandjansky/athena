column host format a40
select user,
       SYS_CONTEXT('USERENV','OS_USER')||'@'||SYS_CONTEXT('USERENV','HOST') host from dual;
