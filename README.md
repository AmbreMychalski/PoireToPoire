# Compile
`make`

# Execute

## Server
`./server_ex <IP_ADRESSE>`

## Client 
`./client_ex <IP_ADRESSE> <name>`

# Functionalities

## Send message to groupe:
`#Send #Group #NameOfGroup message`

## Send a private message :
`#Send #NameOfPerson message`

## Send a message to everyone:
`#Send #all message`

## Create a Group
`#Create #NameOfGroup person1 person2 person3`

comment : the client doesn't need to add his name, he will be automaticaly added to the group

## Add someone to an existing group
`#Add #NameOfGroup person`

## Remove someone to an existing group
`#Remove #NameOfGroup person`

## Send a file to someone
`#Send #file #NameOfPerson NameOfFile`  

comment : the person has to be connected
