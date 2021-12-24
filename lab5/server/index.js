const express = require('express');
const path = require('path');

const PORT = 8080;


const app = express();
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, '/index.html'));
});

const start = () => {
    try {
        app.listen(PORT, () => {
            console.log('Server started on port', PORT);
        })
    } catch (e) {
        console.log(e);
    }
}


start();
