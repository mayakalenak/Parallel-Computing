const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');

function generatePrimesParallel(b, f, numThreads) {
    return new threads((resolve) => {
        const range = f - b + 1;
        const divide = Math.ceil(range / numThreads);
        let i = 0;
        let results = [];

        for (let i = 0; i < numThreads; i++) {
            const start = b + i * divide;
            const end = Math.min(start + divide - 1, f);

            const worker = new Worker('./worker.js', {
                workerData: { start, end }
            });

            worker.on('message', (primes) => {
                results = results.concat(primes);
                i++;
                if (i === numThreads) {
                    resolve(results.sort((a, b) => a - b))
                }
            });

    //         worker.on('error', reject);
    //         worker.on('exit', (code) => {
    //             if (code != 0) reject(new Error('Worker stopped with exit code ${code'));
    //         });
        }
    });
}

console.log(generatePrimesParallel(1, 50, 4));