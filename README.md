# Implant - Post-Exploitation Agent

![Platform](https://img.shields.io/badge/Platform-Windows%20x64%2Fx86-blue.svg)
![Language](https://img.shields.io/badge/Language-C%2B%2B-lightgrey.svg)
![Type](https://img.shields.io/badge/Type-First--Stage%20Dropper-red.svg)

## 📖 Visão Geral

O **Implant** é um agente de pós-exploração focado em técnicas de evasão, persistência e execução de reverse shells. Desenvolvido em C++, o projeto utiliza chamadas diretas a APIs nativas do Windows e resolução dinâmica para evitar detecção estática e dificultar a análise por ferramentas de segurança.

## ⚙️ Capacidades e Técnicas Mapeadas

Este artefato implementa as seguintes técnicas (mapeadas para o framework MITRE ATT&CK):

### 1. Anti-Análise & Anti-Debugging (T1622)
- **PEB Reading:** A função `isDb()` verifica ativamente se o processo está sendo analisado verificando a flag `BeingDebugged` diretamente no *Process Environment Block* (PEB), suportando arquiteturas x86 e x64.
- Caso um debugger seja detectado, a rotina de autodestruição é acionada instantaneamente.

### 2. Self-Deletion via ADS (T1070.004)
- **Rotina `SelfKurtCobain`:** Utiliza *Alternate Data Streams* (ADS). O arquivo renomeia a si mesmo para o fluxo `:Mommy` e emite os sinalizadores `FileRenameInfo` e `FileDispositionInfo` (via `SetFileInformationByHandle`) para excluir o executável do disco enquanto o processo principal ainda roda na memória.

### 3. Persistência de Registro (T1547.001)
- **Rotinas `RakunaMatata`, `Hug` e `Ce`:** O malware se copia de forma silenciosa para o diretório de Links do usuário (`%USERPROFILE%\\Links\\muslc.exe`).
- Em seguida, estabelece persistência criando a chave de registro `Mommy` em `HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run`.

### 4. Ofuscação de Strings (T1027)
- **Rotina `rox`:** Todas as strings sensíveis (nomes de APIs, caminhos de registro, endereço IP e executáveis do sistema) estão armazenadas em arrays de bytes ofuscados e são resolvidas em tempo de execução através de decodificação XOR com a chave `0xFF`.

### 5. Reverse TCP Shell (T1059.003)
- **Rotina `ToRaccon`:** Estabelece comunicação direta (Reverse Shell) com o endereço de C2 configurado na porta `6215`. 
- Redireciona de forma imperceptível o `STDIN`, `STDOUT` e `STDERR` de um novo processo `cmd.exe` alocado, enviando o controle do host diretamente pelo socket aberto.

---

## 🚦 Fluxo de Execução Principal

```text
[Início do Processo]
       |
       v
 [Debugger Detectado? (isDb)]
       |
       |---> [SIM] ---> Aciona Self-Deletion (ADS) ---> [Encerra]
       |
       |---> [NÃO] ---> O processo já está instalado em %USERPROFILE%\\Links ?
                           |
                           |---> [NÃO] ---> Copia o arquivo (RakunaMatata)
                           |                Insere no Registro HKCU (Hug)
                           |                Abre Reverse Shell (ToRaccon)
                           |
                           |---> [SIM] ---> Abre Reverse Shell (ToRaccon)
