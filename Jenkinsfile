

pipeline {
    agent any
    
    parameters {
        string(name: 'VERSION', defaultValue: '2.3.9', description: '')
    }

    stages {        
        stage('build linux-arm64-agent') {
            agent {
                dockerfile {
                    filename 'Dockerfile'                    
                    label 'linux-arm64-agent'
                }
            } 
            steps {
                sh "rm *.gz || echo clean"
                sh "make clean release"
                stash includes: "*.gz", name: 'linux-arm64-agent'
            }
            
        }

        stage('build linux-amd64-agent') {
            agent {
                dockerfile {
                    filename 'Dockerfile'                    
                    label 'linux-amd64-agent'
                }
            } 
            steps {
                sh "rm *.gz || echo clean"
                sh "make clean release"
                stash includes: "*.gz", name: 'linux-amd64-agent'
            }
            
        }

        stage('build mac-arm64-agent') {
            agent { label 'mac-arm64-agent' }                
            steps {
                sh "rm *.zip || echo clean"
                sh "export FAT_BINARY=true && export JAVA_HOME=/opt/homebrew/opt/openjdk/ && make clean release"
                stash includes: "*.zip", name: 'mac-arm64-agent'
            }
            
        }
        stage('commit & publish') {
            agent { label 'linux-arm64-agent' }
            steps {                
                unstash 'linux-arm64-agent'
                unstash 'linux-amd64-agent'
                unstash 'mac-arm64-agent'
                sh '''
                    sed -i  "s/^PROFILER_VERSION=.*/PROFILER_VERSION=$VERSION/" Makefile 
                    git config user.name "Pyroscope Bot"
                    git config user.email "dmitry+bot@pyroscope.io"
                    git checkout $GIT_BRANCH && git add Makefile && git commit -m \"version $VERSION\" && git tag v$VERSION
                '''
                
                withCredentials([string(credentialsId: 'github_token', variable: 'TOKEN')]) {
                    sh '''
                        export U=https://pyroscopebot:$TOKEN@github.com/pyroscope-io/async-profiler.git
                        echo $U 
                        git push $U $GIT_BRANCH && git push $U --tags
                    '''
                }
                
                sh "ls -ltr"
            }
            
        }
        
    }
}

